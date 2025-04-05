'use client'

import { zodResolver } from '@hookform/resolvers/zod'
import { useForm } from 'react-hook-form'
import * as z from 'zod'

import { useToast } from '@/hooks/use-toast'

import {
    Form,
    FormControl,
    FormDescription,
    FormField,
    FormItem,
    FormLabel,
    FormMessage
} from '@/components/ui/form'
import { Button } from "@/components/ui/button"
import { Input } from "@/components/ui/input"
import {
    Select,
    SelectContent,
    SelectItem,
    SelectTrigger,
    SelectValue,
} from "@/components/ui/select"
import { Switch } from "@/components/ui/switch"
import { updateTask } from '@/actions'
import { useState } from 'react'

type TData = {
    id: string
    task_type: string
    task_date: string
    task_time: string
    trigger_type: string
    status: string
    createdAt: string
}

const formSchema = z.object({
    time: z.string({
        message: "Please select a time to config.",
    }),
    type: z.string({
        message: "Please select a type to config.",
    }),
    trigger_type: z.boolean(),
    dot: z.any()
})

export const TaskUpdateForm = ({ updatedData, taskId, userId, loadTask }: { updatedData: TData, taskId: string, userId: string, loadTask: any }) => {
    const { toast } = useToast()

    const form = useForm<z.infer<typeof formSchema>>({
        resolver: zodResolver(formSchema),
        defaultValues: {
            time: updatedData.task_time,
            type: updatedData.task_type,
            trigger_type: updatedData.trigger_type === 'true' ? true : false,
            dot: new Date(updatedData.task_date)
        }
    })

    async function onSubmit(values: z.infer<typeof formSchema>) {
        // Do something with the form values.
        // ✅ This will be type-safe and validated.
        const { time, type, trigger_type, dot } = values
        if (dot && time) {
            const task = {
                task_date: new Date(modifiedDate),
                task_time: time,
                task_type: type,
                trigger_type: trigger_type
            }

            const { data, error } = await updateTask(taskId, userId, task)
            if (error) {
                console.log('error >>>>>', error)
            } else {
                toast({
                    title: "You submitted the following values:",
                    description: (
                        <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
                            <p className="text-white">Submitted Successfully</p>
                        </pre>
                    ),
                })
                loadTask()
            }
        }
    }

    const [modifiedDate, setDate] = useState<string>(updatedData.task_date)

    return (
        <Form {...form}>
            <form onSubmit={form.handleSubmit(onSubmit)} className="space-y-8">
                <FormField
                    control={form.control}
                    name="type"
                    render={({ field }) => (
                        <FormItem>
                            <FormLabel>Components</FormLabel>
                            <Select onValueChange={field.onChange} defaultValue={field.value}>
                                <FormControl>
                                    <SelectTrigger>
                                        <SelectValue placeholder="Select a component to add task" />
                                    </SelectTrigger>
                                </FormControl>
                                <SelectContent>
                                    <SelectItem value="power">Power</SelectItem>
                                    <SelectItem value="relay">Relay</SelectItem>
                                </SelectContent>
                            </Select>
                            <FormDescription>
                                Components in devices
                            </FormDescription>
                            <FormMessage />
                        </FormItem>
                    )}
                />
                <FormField
                    control={form.control}
                    name="trigger_type"
                    render={({ field }) => (
                        <FormItem className="flex flex-row items-center justify-between">
                            <div className="space-y-0.5">
                                <FormLabel className="text-base">
                                    Status
                                </FormLabel>
                                <FormDescription>
                                    Status of Component (On or off)
                                </FormDescription>
                            </div>
                            <FormControl>
                                <Switch
                                    checked={field.value}
                                    onCheckedChange={field.onChange}
                                />
                            </FormControl>
                        </FormItem>
                    )}
                />
                <FormField
                    control={form.control}
                    name="dot"
                    render={({ field }) => (
                        <FormItem className="flex flex-col">
                            <FormLabel>Date of Task</FormLabel>
                            <FormControl>
                                <Input type='date' {...field} value={modifiedDate} onChange={(event) => {
                                    console.log('event >>>>>', event.target.value)
                                    setDate(event.target.value)
                                }} />
                            </FormControl>
                            <FormDescription>
                                Date of specific task to invoke
                            </FormDescription>
                            <FormMessage />
                        </FormItem>
                    )}
                />
                <FormField
                    control={form.control}
                    name="time"
                    render={({ field }) => (
                        <FormItem>
                            <FormLabel>Time</FormLabel>
                            <FormControl>
                                <Input type='time' {...field} />
                            </FormControl>
                            <FormDescription>
                                Choose the specific time to invoke task
                            </FormDescription>
                            <FormMessage />
                        </FormItem>
                    )}
                />
                <Button type="submit">Submit</Button>
            </form>
        </Form>
    )
}
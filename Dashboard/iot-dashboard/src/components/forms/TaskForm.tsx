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
import { addTask } from '@/actions'
import { Dispatch, SetStateAction, useState } from 'react'

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

export const TaskForm = ({ date, setDate, isSetOpen }: { date: Date | undefined, isSetOpen: Dispatch<SetStateAction<boolean>>, setDate: Dispatch<SetStateAction<Date | undefined>> }) => {
    const { toast } = useToast()
    const [year, setYear] = useState<number>(0)
    const [month, setMonth] = useState<string>('')
    const [day, setDay] = useState<string>('')

    const form = useForm<z.infer<typeof formSchema>>({
        resolver: zodResolver(formSchema),
        defaultValues: {
            trigger_type: false,
            dot: date
        }
    })

    async function onSubmit(values: z.infer<typeof formSchema>) {
        // Do something with the form values.
        // ✅ This will be type-safe and validated.
        const { time, type, trigger_type, dot } = values
        if (dot && time) {
            const task = {
                task_date: new Date(formattingDate(date as Date)),
                task_time: time,
                task_type: type,
                trigger_type: trigger_type,
                status: "pending"
            }

            const { data, error } = await addTask(task)
            if (error) {
                console.log('error >>>>>', error)
            }
            else {
                toast({
                    title: "You submitted the following values:",
                    description: (
                        <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
                            <p className="text-white">Submitted Successfully</p>
                        </pre>
                    ),
                })
            }
        }

        setDate(undefined)
        isSetOpen(false)
    }

    const formattingDate = (date: Date) => {
        if (!date) return `${year}-${month}-${day}`

        setYear(date.getFullYear())
        setMonth(String(date.getMonth() + 1).padStart(2, "0")) // Months are zero-based
        setDay(String(date.getDate()).padStart(2, "0"))

        return `${year}-${month}-${day}`
    }

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
                                <Input type='date' {...field} value={formattingDate(date as Date)} readOnly />
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
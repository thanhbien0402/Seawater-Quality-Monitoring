"use client"

import { zodResolver } from "@hookform/resolvers/zod"
import { useForm } from "react-hook-form"
import { z } from "zod"

import { Button } from "@/components/ui/button"
import {
    Form,
    FormControl,
    FormDescription,
    FormField,
    FormItem,
    FormLabel,
    FormMessage,
} from "@/components/ui/form"
import { Input } from "@/components/ui/input"
import { addDevice } from "@/actions"
import { useToast } from "@/hooks/use-toast"
import { useContext, useTransition } from "react"
import { SelectedDeviceContext } from "../context/SelectedDeviceProvider"
import { Icons } from '@/components/icons'
import { cn } from "@/lib/utils"

const formSchema = z.object({
    name: z.string().min(2, {
        message: "Device name must be at least 2 characters.",
    }),
    token: z.string().min(5, {
        message: "Access token must be at least 5 characters.",
    }),
})

export function DeviceForm({ setIsOpen }) {
    const { toast } = useToast()
    const [isPending, startTransition] = useTransition()
    const { refreshDeviceList } = useContext(SelectedDeviceContext)

    const form = useForm<z.infer<typeof formSchema>>({
        resolver: zodResolver(formSchema),
        defaultValues: {
            name: "",
            token: ""
        },
    })

    // 2. Define a submit handler.
    function onSubmit(values: z.infer<typeof formSchema>) {
        // Do something with the form values.
        // ✅ This will be type-safe and validated.
        startTransition(async () => {
            const { name, token: accessToken } = values

            const token = JSON.parse(JSON.stringify(localStorage.getItem('token')))

            const res = await addDevice(name, token, accessToken) //6s24fw1p4qdznfedunx2
            refreshDeviceList()
            // if (error) {
            //     console.log('error >>>>>', error)
            // }
            // else {
            console.log('data >>>>', res)
            toast({
                title: "You submitted the following values:",
                description: (
                    <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
                        <p className="text-white">Submitted Successfully</p>
                    </pre>
                ),
            })

            // const insertData = {
            //     ownerId: res.tenantId.id,
            //     entityType: res.id.entityType,
            //     entityId: res.id.id
            // }

            // const data = await createSettingv2(insertData)
            form.reset()
            setIsOpen(false)
        })
        // }
    }

    return (
        <Form {...form}>
            <form onSubmit={form.handleSubmit(onSubmit)} className="space-y-8">
                <FormField
                    control={form.control}
                    name="name"
                    render={({ field }) => (
                        <FormItem>
                            <FormLabel>Device Name</FormLabel>
                            <FormControl>
                                <Input placeholder="shadcn" {...field} />
                            </FormControl>
                            <FormDescription>
                                This is your public device name.
                            </FormDescription>
                            <FormMessage />
                        </FormItem>
                    )}
                />
                <FormField
                    control={form.control}
                    name="token"
                    render={({ field }) => (
                        <FormItem>
                            <FormLabel>Device MAC Address</FormLabel>
                            <FormControl>
                                <Input placeholder="123456" {...field} />
                            </FormControl>
                            <FormDescription>
                                This is your device MAC Address
                            </FormDescription>
                            <FormMessage />
                        </FormItem>
                    )}
                />
                <Button type="submit">Submit <Icons.spinner
                    className={cn('animate-spin', { hidden: !isPending })}
                /></Button>
            </form>
        </Form>
    )
}
